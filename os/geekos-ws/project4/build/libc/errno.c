const char *__strerrTable[] = {
    "No error", /*  */
    "Unspecified error", /* EUNSPECIFIED */
    "No such file or directory", /* ENOTFOUND */
    "Operation not supported", /* EUNSUPPORTED */
    "No such device", /* ENODEV */
    "Input/output error", /* EIO */
    "Resource in use", /* EBUSY */
    "Out of memory", /* ENOMEM */
    "No such filesystem", /* ENOFILESYS */
    "Name too long", /* ENAMETOOLONG */
    "Invalid format for filesystem", /* EINVALIDFS */
    "Permission denied", /* EACCESS */
    "Invalid argument", /* EINVALID */
    "File descriptor table full", /* EMFILE */
    "Not a directory", /* ENOTDIR */
    "File or directory already exists", /* EEXIST */
    "Out of space on device", /* ENOSPACE */
    "Pipe has no reader", /* EPIPE */
    "Invalid executable format", /* ENOEXEC */
};
const int __strerrTableSize = sizeof(__strerrTable) / sizeof(const char *);
