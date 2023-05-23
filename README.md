# Pixels

[![Elixir CI](https://github.com/arjan/pixels/actions/workflows/elixir.yml/badge.svg)](https://github.com/arjan/pixels/actions/workflows/elixir.yml)
[![Hex pm](https://img.shields.io/hexpm/v/pixels.svg?style=flat)](https://hex.pm/packages/pixels)

Elixir NIF to read and write image data from/to PNG and JPEG files.

For PNG images, it uses the [lodepng][lodepng] C library; for JPEG
images, it uses the [ujpeg][ujpeg] C library.

> Progressive or lossless JPEG files are not supported by ujpeg.
> JPEG encoding not yet supported

[lodepng]: https://lodev.org/lodepng/
[ujpeg]: https://svn.emphy.de/nanojpeg/trunk/ujpeg/

## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed
by adding `pixels` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:pixels, "~> 0.1.0"}
  ]
end
```

Documentation can be generated with [ExDoc](https://github.com/elixir-lang/ex_doc)
and published on [HexDocs](https://hexdocs.pm). Once published, the docs can
be found at [https://hexdocs.pm/pixels](https://hexdocs.pm/pixels).
