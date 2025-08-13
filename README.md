<div align="center">
    <img src="img/logo.svg" width="200px">
    <h1>clid - Command line Color Picker</h1>
</div>

`clid` allows you to view, choose and convert colors in RGB and HEX formats using a simple tui.

## Features
- No external librarys needed.
- TUI Coose color dialog.
- View RGB and HEX colors.
- Change TUI scaling.
- Use output in your own scripts or tools.
- Suports RGB, HEX, HSL, CMYK

## Usage

```shell
# Run clid in normal mode. This lets you choose a color and outputs it to stdout on quit.
$ clid

# Same as before but output in cmyk format.
$ clid --format=cmyk

# This is an example how to pipe the output of clid into wl-copy
$ ./clid --format=hex | tee /dev/tty | tail -n 1 | wl-copy

# Use --help to get a list of all arguments and view tui inputs.
$ clid --help
```

## Build & Install
```shell
# Clone clid repo
$ git clone https://github.com/iinsertNameHere/clid.git
$ cd clid

# Build using make
$ make

# Install using make
$ make install

# Run program
$ clid
```

#### Uninstall:
```shell
$ make uninstall
```

## Showcase
<img src="img/screenshot.png" width="400">