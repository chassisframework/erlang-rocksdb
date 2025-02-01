

# erlang-rocksdb - Erlang wrapper for RocksDB. #

:[![Build Status](https://github.com/EnkiMultimedia/erlang-rocksdb/workflows/build/badge.svg)](https://github.com/EnkiMultimedia/erlang-rocksdb/actions?query=workflow%3Abuild)
[![Hex pm](http://img.shields.io/hexpm/v/rocksdb.svg?style=flat)](https://hex.pm/packages/rocksdb)

Copyright (c) 2016-2025 Benoît Chesneau

Feedback and pull requests welcome! If a particular feature of RocksDB is important to you, please let me know by opening an issue, and I'll prioritize it.

## Features

- rocksdb 9.10.0 with snappy 1.12.1, lz4 1.8.3
- Erlang 22 and sup with dirty-nifs enabled
- all basics db operations
- batchs support
- snapshots support
- checkpoint support
- column families support
- transaction logs
- backup support
- erlang merge operator
- customized build support
- Tested on macosx, freebsd, solaris and linux

## Usage

See the [Doc](https://hexdocs.pm/rocksdb/) for more explanation.

> Note: since the version **0.26.0**, `cmake>=3.4` is required to install `erlang-rocksdb`.

## Customized build ##

See the [Customized builds](https://hexdocs.pm/rocksdb/CUSTOMIZED_BUILDS.html) for more information.

## Support

Support, Design and discussions are done via the [Github Tracker](https://github.com/EnkiMultimedia/erlang-rocksdb/issues).

## License

Erlang RocksDB is licensed under the Apache License 2.


## Modules ##


<table width="100%" border="0" summary="list of modules">
<tr><td><a href="http://gitlab.com/barrel-db/erlang-rocksdb/blob/master/doc/rocksdb.md" class="module">rocksdb</a></td></tr></table>

