# PC performance measurements

## How to compile

Simply run `make` under the root directory, and the compiled binaries will be
generated under `build` directory for each group of measurements.

## How to run

Most of binaries can be directly run. But there are several exceptions.

### Network

All senders needs to be started after the corresponding receivers have been
started.

### File System

For file-cache test, firstly you needs to run `./filesystem/file-cache-preprocess.py` to
generate corresponding files to measure the file cache size. However, this
script will use up a large amount of disk space under `$HOME/tmp-files/` (around
32G). Then to generate a 32GB file for the final test in file-cache, you needs
to manually run the following script **under** `$HOME/tmp-files/`.

```bash
dd if=/dev/zero of=./15 bs=1G count=32
```

For other two tests, simply run `./filesystem/file-read-preprocess.py` and
`./filesystem/contention-preprocess.py` to generate testing files.

For nfs test, you needs to mount the nfs directory under `./filesystem/build/nfs-files/`, and copy all files under `./filesystem/build/file-read/` into this directory before run the test.
