#ifndef i18n_h
#define i18n_h

#include <libintl.h>
#if 0
 #define _(string)                         (string)
 #define N_(string)                        (string)
 #define textdomain(domain)
 #define bindtextdomain(package,directory)
#else
 #define _(string)                         gettext(string)
 #define gettext_noop(string)              (string)
 #define N_(string)                        gettext_noop(string)
#endif /* if 1 */


#endif /* i18n_h */
