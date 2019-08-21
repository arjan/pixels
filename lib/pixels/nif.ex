defmodule Pixels.Nif do
  @moduledoc false
  @on_load :load_nif

  def load_nif do
    file = :filename.join(:code.priv_dir(:pixels), 'pixels_nif')
    :ok = :erlang.load_nif(file, 0)
  end

  def read_png_file(_filename) do
    raise "NIF read_png_file/1 not implemented"
  end

  def read_png_buffer(_buffer) do
    raise "NIF read_png_buffer/1 not implemented"
  end
end
