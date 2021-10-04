"""Script which downloads the build dependencies."""

import os
import platform
import zipfile

import requests


DOXYGEN = "https://doxygen.nl/files/doxygen-1.9.1.windows.x64.bin.zip"


def _download_and_unzip(download_uri, name, output_dir="."):
    ci_dir = os.path.dirname(__file__)
    zip_path = os.path.join(ci_dir, name + ".zip")

    if not os.path.exists(zip_path):
        print("Downloading", name, "from", download_uri)
        response = requests.get(download_uri)
        with open(zip_path, "wb") as file:
            file.write(response.content)

    print("Extracting", name)
    with zipfile.ZipFile(zip_path, "r") as zip_ref:
        zip_ref.extractall(os.path.join(ci_dir, output_dir))

    print("Done.")


def _main():
    ci_dir = os.path.dirname(__file__)

    if platform.system() == "Windows" and not os.path.exists(os.path.join(ci_dir, "doxygen", "doxygen.exe")):
        _download_and_unzip(DOXYGEN, "doxygen", "doxygen")


if __name__ == "__main__":
    _main()
