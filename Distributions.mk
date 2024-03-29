OS_RELEASE_FILE = /etc/os-release
ifneq (,$(wildcard, $(OS_RELEASE_FILE)))
$(error cannot find /etc/os-release.  This Makefile look up the file to get distribution name and version.)
endif

OS      = $(shell . /etc/os-release; echo -n $$ID)
VERSION = $(shell . /etc/os-release; echo -n $$VERSION_ID)
MAJOR_VERSION = $(shell echo $(VERSION) | sed -e 's/\..*//')
MINOR_VERSION = $(shell echo $(VERSION) | awk -F'.' '{print $$2}')

# If the distribution is CentOS  7, We need minor version to determine python version
ifeq ($(strip $(OS)), centos)
ifeq ($(strip $(VERSION)), 7)
MINOR_VERSION = $(shell awk -F'[ |.]' '{print $$5}' /etc/redhat-release)
endif
endif

# default vars
USE_MOD_PYTHON = 0
USE_MOD_WSGI   = 0
USE_PYTHON2    = 0
USE_PYTHON3    = 0

########## Scientific Linux 5.0 - 5.7 ##########
# Scientific Linux older than or equal to 5.7 returns ScientificSL
# Scientific Linux newer than or equal to 5.8 returns Scientific

ifeq ($(strip $(OS)),ScientificSL)
    WWW_DOCUMENT_ROOT = /var/www/html
    HTTPD_CONF_DIR    = /etc/httpd/conf.d
    USE_MOD_PYTHON    = 1
endif

########## Scientific Linux 5.8 - , 7.x ##########
ifeq ($(strip $(OS)),scientific)
    WWW_DOCUMENT_ROOT = /var/www/html
    HTTPD_CONF_DIR    = /etc/httpd/conf.d
    ifeq ($(strip $(MAJOR_VERSION)),5)
        USE_MOD_PYTHON    = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python2-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),6)
        USE_MOD_WSGI      = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),7)
        USE_MOD_WSGI      = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python2-config
        ifeq ($(shell test $(MINOR_VERSION) -gt 6; echo $$?),0)
            USE_PYTHON2      = 0
            USE_PYTHON3      = 1
            PYTHON_EXEC_FILE = python3
            PYTHON_CONFIG    = python3-config
        endif
    endif
endif

########## Scientific Linux CERN ##########
ifeq ($(strip $(OS)),ScientificCERNSLC)
    WWW_DOCUMENT_ROOT = /var/www/html
    HTTPD_CONF_DIR    = /etc/httpd/conf.d
    ifeq ($(strip $(MAJOR_VERSION)),5)
        USE_MOD_PYTHON    = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python2-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),6)
        USE_MOD_WSGI      = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python2-config
    endif
endif
# CERN CentOS 7 is like a CentOS 7

########## CentOS ##########
ifeq ($(strip $(OS)),centos)
WWW_DOCUMENT_ROOT = /var/www/html
HTTPD_CONF_DIR    = /etc/httpd/conf.d
    ifeq ($(strip $(MAJOR_VERSION)),5)
        USE_MOD_PYTHON    = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python2-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),6)
        USE_MOD_WSGI      = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),7)
        USE_MOD_WSGI      = 1
        USE_PYTHON2       = 1
        PYTHON_EXEC_FILE  = python2
        PYTHON_CONFIG     = python2-config
        # Use python3 on CentOS 7.7 and later
        ifeq ($(shell test $(MINOR_VERSION) -gt 6; echo $$?),0)
            USE_PYTHON2 = 0
            USE_PYTHON3 = 1
            PYTHON_EXEC_FILE  = python3
            PYTHON_CONFIG     = python3-config
        endif
    endif
    ifeq ($(strip $(MAJOR_VERSION)),8)
        USE_MOD_WSGI      = 1
        USE_PYTHON3       = 1
        PYTHON_EXEC_FILE  = python3
        PYTHON_CONFIG     = python3-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),9)
        USE_MOD_WSGI      = 1
        USE_PYTHON3       = 1
        PYTHON_EXEC_FILE  = python3
        PYTHON_CONFIG     = python3-config
    endif
endif

########## AlmaLinux ##########
ifeq ($(strip $(OS)),almalinux)
WWW_DOCUMENT_ROOT = /var/www/html
HTTPD_CONF_DIR    = /etc/httpd/conf.d
    ifeq ($(strip $(MAJOR_VERSION)),8)
        USE_MOD_WSGI      = 1
        USE_PYTHON3       = 1
        PYTHON_EXEC_FILE  = python3
        PYTHON_CONFIG     = python3-config
    endif
    ifeq ($(strip $(MAJOR_VERSION)),9)
        USE_MOD_WSGI      = 1
        USE_PYTHON3       = 1
        PYTHON_EXEC_FILE  = python3
        PYTHON_CONFIG     = python3-config
    endif
endif

########## Fedora ##########
ifeq ($(strip $(OS)),Fedora)
    WWW_DOCUMENT_ROOT = /var/www/html
    HTTPD_CONF_DIR    = /etc/httpd/conf.d
    USE_MOD_WSGI      = 1
    USE_PYTHON3       = 1
    PYTHON_EXEC_FILE  = python3
    PYTHON_CONFIG     = python3-config
endif

########## Debian ##########
ifeq ($(strip $(OS)),debian)
    WWW_DOCUMENT_ROOT = /var/www
    HTTPD_CONF_DIR    = /etc/apache2/conf.d
    USE_MOD_WSGI      = 1
    USE_PYTHON3       = 1
    PYTHON_EXEC_FILE  = python3
    PYTHON_CONFIG     = python3-config
endif

########## Ubuntu ##########
ifeq ($(strip $(OS)),ubuntu)
    WWW_DOCUMENT_ROOT = /var/www
    HTTPD_CONF_DIR    = /etc/apache2/conf.d
    USE_MOD_WSGI      = 1
    USE_PYTHON3       = 1
    PYTHON_EXEC_FILE  = python3
    PYTHON_CONFIG     = python3-config
endif

########## ArchLinux  ##########
ifeq ($(strip $(OS)),arch)
    WWW_DOCUMENT_ROOT = /srv/http
    HTTPD_CONF_DIR    = /etc/httpd/conf/extra
    USE_MOD_WSGI      = 1
    USE_PYTHON3       = 1
    PYTHON_EXEC_FILE  = python
    PYTHON_CONFIG     = python-config
endif

########## Raspbian ##########
ifeq ($(strip $(OS)),raspbian)
    WWW_DOCUMENT_ROOT = /var/www/html
    HTTPD_CONF_DIR    = /etc/apache2/conf-available
    USE_MOD_WSGI      = 1
    USE_PYTHON3       = 1
    PYTHON_EXEC_FILE  = python3
    PYTHON_CONFIG     = python3-config
endif

########## Unknown ##########
ifeq ($(strip $(WWW_DOCUMENT_ROOT)),)
    WWW_DOCUMENT_ROOT = /
    HTTPD_CONF_DIR    = /
    USE_MOD_WSGI      = 1
endif

PYTHON_EXEC_PATH   = /usr/bin/$(PYTHON_EXEC_FILE)
PYTHON_CONFIG_PATH = /usr/bin/$(PYTHON_CONFIG)

# all target for testing #
#all:
#       @echo "os:" $(OS)
#       @echo "version:" $(VERSION)
#       @echo "arch:" $(ARCH)
#       @echo "major_version:" $(MAJOR_VERSION)
#       @echo "httpd_document_root:" $(HTTPD_DOCUMENT_ROOT)
#       @echo "mod_python:" $(USE_MOD_PYTHON)
#       @echo "mod_wsgi:" $(USE_MOD_WSGI)
