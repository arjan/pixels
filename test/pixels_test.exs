defmodule PixelsTest do
  use ExUnit.Case
  doctest Pixels

  test "file not found" do
    assert {:error, :enoent} = Pixels.read_file("test/fxxx.png")
  end

  test "file invalid" do
    File.write!("/tmp/t", "asdf")
    assert {:error, :invalid_data} = Pixels.read_file("/tmp/t")
  end

  test "reads a PNG file" do
    test_dot("test/images/dot.png")
  end

  test "reads a mono JPG file" do
    test_dot("test/images/dot.jpg")
  end

  test "reads a RGB JPG file" do
    test_dot("test/images/dot_rgb.jpg")
  end

  test "encode a PNG" do
    pixels = %Pixels{
      width: 2,
      height: 2,
      data: <<255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 255, 0, 0, 0, 255>>
    }

    assert {:ok, data} = Pixels.encode_png(pixels)

    assert :png = Pixels.Identify.identify(data)

    assert {:ok, pixels} == Pixels.read(data)
  end

  defp test_dot(filename) do
    assert {:ok, %Pixels{width: 8, height: 8, data: data}} = Pixels.read_file(filename)

    assert 4 * 8 * 8 == byte_size(data)

    assert <<r1, g1, b1, 255, _r2, _g2, _b2, 255, _::binary>> = data
    assert r1 < 10
    assert g1 < 10
    assert b1 < 10
  end
end
