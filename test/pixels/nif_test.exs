defmodule Pixels.NifTest do
  use ExUnit.Case

  test "read_jpeg_file" do
    s = 8 * 8 * 4 * 8
    data = File.read!("test/dot.jpg")
    assert {8, 8, <<_::size(s)>>} = Pixels.Nif.decode_jpeg(data)
  end
end
