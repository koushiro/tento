# tento

A C++14 network library based on [muduo](https://github.com/chenshuo/muduo).

## Features

- Modern C++.
- Reactor pattern (one loop per thread + threadpool).
- Linux supported only. 
- Timer supported.
- Nonblocking multi-thread TCP server. (Unfinished yet)

## Project structure

```bash
├── deps        # third party dependencies
│   ├── fmt		  
│   ├── googletest
│   └── spdlog
├── examples    # exmaple source code.
├── target      # store target files (executable and library files).
│   ├── bin	# executable binary directory (tests and examples).
│   └── lib	# static library diretory.
├── tento       # tento source code.
│   ├── base
│   └── net
└── tests       # test source code.
```

## Build

```bash
git clone --recursive https://github.com/koushiro/tento.git
cmake .
make
```

## Tests


## Examples


## TODO

- Nonblocking TCP client.
- Nonblocking multi-thread HTTP server.
- Nonblocking HTTP client.

## LICENSE

MIT
