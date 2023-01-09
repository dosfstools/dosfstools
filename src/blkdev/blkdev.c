/*
 * No copyright is claimed.  This code is in the public domain; do with
 * it what you wish.
 *
 * Written by Karel Zak <kzak@redhat.com>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#ifdef HAVE_SYS_SYSMACROS_H
#include <sys/sysmacros.h>
#endif

#ifdef HAVE_LINUX_HDREG_H
#include <linux/hdreg.h>
#endif

#ifdef HAVE_LINUX_FD_H
#include <linux/fd.h>
#endif

#ifdef HAVE_SYS_DISKLABEL_H
#include <sys/disklabel.h>
#endif

#ifdef HAVE_SYS_DISK_H
# ifdef HAVE_SYS_QUEUE_H
#  include <sys/queue.h>	/* for LIST_HEAD */
# endif
# include <sys/disk.h>
#endif

#include "blkdev.h"
#include "linux_version.h"

static long
blkdev_valid_offset (int fd, off_t offset) {
	char ch;

	if (lseek (fd, offset, 0) < 0)
		return 0;
	if (read (fd, &ch, 1) < 1)
		return 0;
	return 1;
}

int is_blkdev(int fd)
{
	struct stat st;
	return (fstat(fd, &st) == 0 && S_ISBLK(st.st_mode));
}

off_t
blkdev_find_size (int fd) {
	uintmax_t high, low = 0;

	for (high = 1024; blkdev_valid_offset (fd, high); ) {
		if (high == UINTMAX_MAX)
			return -1;

		low = high;

		if (high >= UINTMAX_MAX/2)
			high = UINTMAX_MAX;
		else
			high *= 2;
	}

	while (low < high - 1)
	{
		uintmax_t mid = (low + high) / 2;

		if (blkdev_valid_offset (fd, mid))
			low = mid;
		else
			high = mid;
	}
	blkdev_valid_offset (fd, 0);
	return (low + 1);
}

/* get size in bytes */
int
blkdev_get_size(int fd, unsigned long long *bytes)
{
#ifdef DKIOCGETBLOCKCOUNT
	/* Apple Darwin */
	if (ioctl(fd, DKIOCGETBLOCKCOUNT, bytes) >= 0) {
		*bytes <<= 9;
		return 0;
	}
#endif

#ifdef BLKGETSIZE64
	{
#ifdef __linux__
		int ver = get_linux_version();

		/* kernels 2.4.15-2.4.17, had a broken BLKGETSIZE64 */
		if (ver >= KERNEL_VERSION (2,6,0) ||
		   (ver >= KERNEL_VERSION (2,4,18) && ver < KERNEL_VERSION (2,5,0)))
#endif
			if (ioctl(fd, BLKGETSIZE64, bytes) >= 0)
				return 0;
	}
#endif /* BLKGETSIZE64 */

#ifdef BLKGETSIZE
	{
		unsigned long size;

		if (ioctl(fd, BLKGETSIZE, &size) >= 0) {
			*bytes = ((unsigned long long)size << 9);
			return 0;
		}
	}

#endif /* BLKGETSIZE */

#ifdef DIOCGMEDIASIZE
	/* FreeBSD */
	if (ioctl(fd, DIOCGMEDIASIZE, bytes) >= 0)
		return 0;
#endif

#ifdef FDGETPRM
	{
		struct floppy_struct this_floppy;

		if (ioctl(fd, FDGETPRM, &this_floppy) >= 0) {
			*bytes = ((unsigned long long) this_floppy.size) << 9;
			return 0;
		}
	}
#endif /* FDGETPRM */

#ifdef DIOCGDINFO
	{
		/*
		 * This code works for FreeBSD 4.11 i386, except for the full device
		 * (such as /dev/ad0). It doesn't work properly for newer FreeBSD
		 * though. FreeBSD >= 5.0 should be covered by the DIOCGMEDIASIZE
		 * above however.
		 *
		 * Note that FreeBSD >= 4.0 has disk devices as unbuffered (raw,
		 * character) devices, so we need to check for S_ISCHR, too.
		 */
		int part = -1;
		struct disklabel lab;
		struct partition *pp;
		struct stat st;

		if ((fstat(fd, &st) >= 0) &&
		    (S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode)))
			part = st.st_rdev & 7;

		if (part >= 0 && (ioctl(fd, DIOCGDINFO, (char *)&lab) >= 0)) {
			pp = &lab.d_partitions[part];
			if (pp->p_size) {
				 *bytes = pp->p_size << 9;
				 return 0;
			}
		}
	}
#endif /* DIOCGDINFO */

	{
		struct stat st;

		if (fstat(fd, &st) == 0 && S_ISREG(st.st_mode)) {
			*bytes = st.st_size;
			return 0;
		}
		if (!S_ISBLK(st.st_mode))
			return -1;
	}

	*bytes = blkdev_find_size(fd);
	return 0;
}

/* get 512-byte sector count */
int
blkdev_get_sectors(int fd, unsigned long long *sectors)
{
	unsigned long long bytes;

	if (blkdev_get_size(fd, &bytes) == 0) {
		*sectors = (bytes >> 9);
		return 0;
	}

	return -1;
}

/*
 * Get logical sector size.
 *
 * This is the smallest unit the storage device can
 * address. It is typically 512 bytes.
 */
int blkdev_get_sector_size(int fd, int *sector_size)
{
#ifdef BLKSSZGET
	if (ioctl(fd, BLKSSZGET, sector_size) >= 0)
		return 0;
	return -1;
#else
	(void)fd; /* prevent unused parameter warning */
	*sector_size = DEFAULT_SECTOR_SIZE;
	return 0;
#endif
}

/*
 * Get physical block device size. The BLKPBSZGET is supported since Linux
 * 2.6.32. For old kernels is probably the best to assume that physical sector
 * size is the same as logical sector size.
 *
 * Example:
 *
 * rc = blkdev_get_physector_size(fd, &physec);
 * if (rc || physec == 0) {
 *	rc = blkdev_get_sector_size(fd, &physec);
 *	if (rc)
 *		physec = DEFAULT_SECTOR_SIZE;
 * }
 */
int blkdev_get_physector_size(int fd, int *sector_size)
{
#ifdef BLKPBSZGET
	if (ioctl(fd, BLKPBSZGET, &sector_size) >= 0)
		return 0;
	return -1;
#else
	(void)fd; /* prevent unused parameter warning */
	*sector_size = DEFAULT_SECTOR_SIZE;
	return 0;
#endif
}

/*
 * Return the alignment status of a device
 */
int blkdev_is_misaligned(int fd)
{
#ifdef BLKALIGNOFF
	int aligned;

	if (ioctl(fd, BLKALIGNOFF, &aligned) < 0)
		return 0;			/* probably kernel < 2.6.32 */
	/*
	 * Note that kernel returns -1 as alignment offset if no compatible
	 * sizes and alignments exist for stacked devices
	 */
	return aligned != 0 ? 1 : 0;
#else
	(void)fd; /* prevent unused parameter warning */
	return 0;
#endif
}

int blkdev_is_cdrom(int fd)
{
#ifdef CDROM_GET_CAPABILITY
	int ret;

	if ((ret = ioctl(fd, CDROM_GET_CAPABILITY, NULL)) < 0)
		return 0;
	else
		return ret;
#else
	(void)fd; /* prevent unused parameter warning */
	return 0;
#endif
}

/*
 * Get kernel's interpretation of the device's geometry.
 *
 * Returns the heads and sectors - but not cylinders
 * as it's truncated for disks with more than 65535 tracks.
 *
 * Note that this is deprecated in favor of LBA addressing.
 */
int blkdev_get_geometry(int fd, unsigned int *h, unsigned int *s)
{
#ifdef HDIO_GETGEO
	{
		struct hd_geometry geometry;

		if (ioctl(fd, HDIO_GETGEO, &geometry) == 0) {
			*h = geometry.heads;
			*s = geometry.sectors;
			return 0;
		}
	}
#endif

#ifdef FDGETPRM
	{
		struct floppy_struct fdparam;

		if (ioctl(fd, FDGETPRM, &fdparam) == 0) {
			*h = fdparam.head;
			*s = fdparam.sect;
			return 0;
		}
	}
#endif

	(void)fd; /* prevent unused parameter warning */
	*h = 0;
	*s = 0;
	return -1;
}

/*
 * Get start offset of partition
 */
int blkdev_get_start(int fd, dev_t rdev, unsigned long long *s)
{
#ifdef __linux__
	{
		char path[PATH_MAX];
		FILE *file;
		int ret;

		snprintf(path, sizeof(path), "/sys/dev/block/%d:%d/start", major(rdev), minor(rdev));
		file = fopen(path, "r");
		if (file) {
			ret = fscanf(file, "%llu", s);
			fclose(file);
			if (ret == 1)
				return 0;
		}
	}
#endif

#ifdef HDIO_GETGEO
	{
	struct hd_geometry geometry;

	if (ioctl(fd, HDIO_GETGEO, &geometry) == 0) {
		*s = geometry.start;
		return 0;
	}
	}
#endif

	(void)rdev; /* prevent unused parameter warning */
	(void)fd; /* prevent unused parameter warning */
	*s = 0;
	return -1;
}

/*
 * Convert scsi type to human readable string.
 */
const char *blkdev_scsi_type_to_name(int type)
{
	switch (type) {
	case SCSI_TYPE_DISK:
		return "disk";
	case SCSI_TYPE_TAPE:
		return "tape";
	case SCSI_TYPE_PRINTER:
		return "printer";
	case SCSI_TYPE_PROCESSOR:
		return "processor";
	case SCSI_TYPE_WORM:
		return "worm";
	case SCSI_TYPE_ROM:
		return "rom";
	case SCSI_TYPE_SCANNER:
		return "scanner";
	case SCSI_TYPE_MOD:
		return "mo-disk";
	case SCSI_TYPE_MEDIUM_CHANGER:
		return "changer";
	case SCSI_TYPE_COMM:
		return "comm";
	case SCSI_TYPE_RAID:
		return "raid";
	case SCSI_TYPE_ENCLOSURE:
		return "enclosure";
	case SCSI_TYPE_RBC:
		return "rbc";
	case SCSI_TYPE_OSD:
		return "osd";
	case SCSI_TYPE_NO_LUN:
		return "no-lun";
	default:
		break;
	}
	return NULL;
}
