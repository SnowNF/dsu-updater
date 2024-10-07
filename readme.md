# DSU Updater

The DSU Updater is designed for partition updates under DSU (Dynamic System Updates).

Currently, it has only been tested for updating `system.img`.

## Build Instructions

On an aarch64 GNU/Linux system, install the required dependencies such as `g++`, `cmake`, `uuid-dev`, `zlib1g-dev`, `libssl-dev`, and others.

Then, run the following commands:

```bash
bash -e init_source.sh
cmake -B build .
cmake --build build -j
```

## Running the Updater

Copy the binary to your terminal (e.g. Termux) and run:

```bash
sudo /path/to/dsu_updater
```

## References



## License

This project is licensed under the GPLv3.