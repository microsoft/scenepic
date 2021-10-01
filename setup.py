"""Setup script for scenepic."""

from distutils.version import LooseVersion
import os
import platform
import re
import subprocess
import sys

from setuptools import Extension, find_packages, setup
from setuptools.command.build_ext import build_ext


REQUIRES = [
    "numpy>=1.16.2",
    "pillow>=5.2.0",
    "scipy>=1.4.1"
]

REQUIRES_VIDEO = [
    "opencv-python"
]

REQUIRES_DEV = [
    "azure-storage-blob>=12.0.0",
    "azure-common",
    "nbstripout",
    "pytest-azurepipelines",
    "pytest-cov",
    "pytest",
    "requests",
    "sphinx",
    "sphinx-autodoc-typehints",
    "sphinx-markdown-builder",
    "twine",
    "wheel"
]

with open("README.md", "r") as file:
    LONG_DESCRIPTION = file.read()

with open("VERSION", "r") as file:
    VERSION = file.read()


class CMakeExtension(Extension):
    """Extension class for cmake."""

    def __init__(self, name: str, source_dir=""):
        """Initializer.

        Args:
            name (str): The extension name
            source_dir (str, optional): The directory containing the source files. Defaults to "".
        """
        Extension.__init__(self, name, sources=[])
        self.source_dir = os.path.abspath(source_dir)


class CMakeBuild(build_ext):
    """Build driver for cmake."""

    def run(self):
        """Runs the build extension."""
        try:
            out = subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r"version\s*([\d.]+)", out.decode()).group(1))
            if cmake_version < "3.1.0":
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext: CMakeExtension):
        """Builds the specified extension.

        Args:
            ext (CMakeExtension): the cmake extension.
        """
        cfg = "Debug" if self.debug else "Release"
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ["-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
                      "-DCMAKE_BUILD_TYPE=" + cfg,
                      "-DPYTHON_EXECUTABLE=" + sys.executable,
                      "-DSCENEPIC_BUILD_PYTHON=1"]

        build_args = ["--config", cfg, "--target", "_scenepic"]

        if platform.system() == "Windows":
            cmake_args += ["-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir)]
            if platform.architecture()[0] == "64bit":
                cmake_args += ["-A", "x64"]
            else:
                cmake_args += ["-A", "Win32"]

            build_args += ["--", "/m"]
        else:
            cmake_args += ["-GUnix Makefiles", "-DCMAKE_BUILD_TYPE=" + cfg]
            build_args += ["--", "-j2"]

        env = os.environ.copy()
        env["CXXFLAGS"] = """{} -DVERSION_INFO="{}" """.format(env.get("CXXFLAGS", ""),
                                                               self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(["cmake", ext.source_dir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(["cmake", "--build", "."] + build_args, cwd=self.build_temp)


setup(
    name="scenepic",
    version=VERSION,
    author="ScenePic Team",
    author_email="scenepic@microsoft.com",
    description="Python visualization library for 3D scenes",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    packages=find_packages("cppsrc"),
    package_dir={"": "cppsrc"},
    ext_modules=[CMakeExtension("scenepic._scenepic")],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent"
    ],
    url="https://github.com/microsoft/scenepic",
    install_requires=REQUIRES,
    extras_require={
        "dev": REQUIRES_DEV,
        "video": REQUIRES_VIDEO
    },
    tests_require=["pytest"],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
