import os
import subprocess

def install_package(package):
    try:
        __import__(package.split('==')[0])
        print(f"{package} is already installed")
    except ImportError:
        print(f"{package} not found, Installing now......")
        subprocess.run(['pip', 'install', package], check=True)
        

def set_environment():        
    dependencies = [
        'backcall==0.2.0', 'beautifulsoup4==4.12.2', 'certifi==2023.5.7', 'filetype==1.2.0',
        'gitdb==4.0.10', 'GitPython==3.1.31', 'numpy==1.24.3', 'openpyxl==3.1.2',
        'pandas==2.0.1', 'pytest==7.3.1', 'python-gitlab==3.14.0',
        'pywin32==306', 'pyzmq==25.0.2', 'repo==0.3.0', 'requests==2.30.0',
        'setuptools==67.7.2', 'zipp==3.15.0', 'generic-path==0.3', 'subprocess32==3.5.4',
        'pyinstaller==5.10.1'
    ]

    for package in dependencies:
        install_package(package)

if __name__ == '__main__':
    set_environment()
