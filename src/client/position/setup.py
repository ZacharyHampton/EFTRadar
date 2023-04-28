from distutils.core import setup, Extension

mymodule = Extension('position', sources=['position.cpp'])

setup(
    name='position',
    version='1.0',
    description='Sample python C-API exploration',
    ext_modules=[mymodule]
)
