gres = find_program('glib-compile-resources')

resources_c = custom_target('gks-resources.c',
  output : 'gks-resources.c',
  input : 'gnome-keysign.gresource.xml',
  command : [gres, '--target', '@OUTPUT@', '--c-name=gks', '--generate-source',
             '--sourcedir=../data', '@INPUT@'],
)

resources_h = custom_target('gks-resources.h',
  output : 'gks-resources.h',
  input : 'gnome-keysign.gresource.xml',
  command : [gres, '--target', '@OUTPUT@', '--c-name=gks', '--generate-header',
             '--sourcedir=../data', '@INPUT@'],
)

install_data(sources : 'gnome-keysign.desktop',
             install_dir : 'share/applications')

install_data(sources : 'gnome-keysign.svg',
             install_dir : 'share/icons/hicolor/scalable/apps')
