gnome = import('gnome')

resources = gnome.compile_resources('gks-resources',
                                    'gnome-keysign.gresource.xml',
                                    source_dir : 'data',
                                    c_name : 'gks')

install_data(sources : 'gnome-keysign.desktop',
             install_dir : 'share/applications')

install_data(sources : 'gnome-keysign.svg',
             install_dir : 'share/icons/hicolor/scalable/apps')
