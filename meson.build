project(
    'pathfs',
    ['c', 'cpp'],
    default_options : ['cpp_std=gnu++20']
)

fuse3 = dependency('fuse3')

libpathfs_inc = include_directories('include')
libpathfs_lib = library('pathfs'
, [ 'lib/utils.cpp' ]
, include_directories: libpathfs_inc
, install: true
)

libpathfs = declare_dependency(link_with: libpathfs_lib,
                               include_directories: libpathfs_inc)



executable('pathfs', ['pathfs.cpp'], dependencies: [ fuse3, libpathfs ], install: true)
