TEMPLATE = subdirs

SUBDIRS += \
    misc \
    widget \
    attr \
    converter \
    core \
    typelist \
    models

# No tests if PREFIX is set (for flatpak)
defined(PREFIX, var) {
    SUBDIRS =
}
