#include <stdint.h>

typedef struct {
	char *TITLE;
	char *URL;
	char **TAGS;
	uint64_t ADD_DATE;
	uint64_t LAST_MODIFIED;
	char *ICON;
	char *ICON_URI;
} BOOKMARK;

