defmodule PixelsTest do
  use ExUnit.Case
  doctest Pixels

  test "file not found" do
    assert {:error, :not_found} = Pixels.read_file("test/fxxx.png")
  end

  test "file invalid" do
    File.write!("/tmp/t", "asdf")
    assert {:error, :invalid_format} = Pixels.read_file("/tmp/t")
  end

  test "reads a PNG file, RGBA" do
    assert {:ok, %Pixels{width: 8, height: 8, data: data}} = Pixels.read_file("test/dot.png")

    assert 4 * 8 * 8 == byte_size(data)
    assert <<0, 0, 0, 255, 0, 0, 0, 255, _::binary>> = data
  end
end
