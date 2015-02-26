glib = dependency('glib-2.0')
gpgme = find_library('gpgme')
gtk = dependency('gtk+-3.0', version : '>=3.12')

add_global_arguments('-DG_LOG_DOMAIN="GnomeKeySign"', language : 'c')

libkeysign = static_library('keysign',
                            'gks-gpg.c', 'gks-gpg.h',
                            dependencies : [glib, gpgme],
                            install : false)

executable('gnome-keysign',
           'gks-main.c', resources_c, resources_h,
           dependencies : [glib, gpgme, gtk],
           link_with : libkeysign)
