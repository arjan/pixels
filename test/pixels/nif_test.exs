defmodule Pixels.NifTest do
  use ExUnit.Case

  test "read_jpeg_file" do
    s = 8 * 8 * 4 * 8
    assert {8, 8, <<_::size(s)>>} = Pixels.Nif.read_jpeg_file("test/dot.jpg")
  end
end
