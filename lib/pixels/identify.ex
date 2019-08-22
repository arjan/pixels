defmodule Pixels.Identify do
  def identify(
        <<0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, _length::size(32), "IHDR",
          _width::size(32), _height::size(32), _bit_depth, _color_type, _compression_method,
          _filter_method, _interlace_method, _crc::size(32), _chunks::binary>>
      ),
      do: :png

  def identify(
        <<0xFF, 0xD8, 0xFF, 0xE0, _length::size(16), "JFIF", 0, _version::size(16), _::binary()>>
      ),
      do: :jpeg

  def identify(_data), do: :unknown
end
