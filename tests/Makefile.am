AM_TESTS_ENVIRONMENT = XXD_FOUND=@XXD_FOUND@; export XXD_FOUND;

TESTS = referenceFAT12.mkfs referenceFAT16.mkfs referenceFAT32.mkfs
TEST_EXTENSIONS = .mkfs
MKFS_LOG_COMPILER = $(srcdir)/test-mkfs

dist_check_DATA = test-mkfs           \
		  referenceFAT12.mkfs \
		  referenceFAT12.xxd  \
		  referenceFAT16.mkfs \
		  referenceFAT16.xxd  \
		  referenceFAT32.mkfs \
		  referenceFAT32.xxd
