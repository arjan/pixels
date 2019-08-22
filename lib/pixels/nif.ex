defmodule Pixels.Nif do
  @moduledoc false
  @on_load :load_nif

  def load_nif do
    file = :filename.join(:code.priv_dir(:pixels), 'pixels_nif')
    :ok = :erlang.load_nif(file, 0)
  end

  def decode_png(_data) do
    raise "NIF decode_png/1 not implemented"
  end

  def decode_jpeg(_data) do
    raise "NIF decode_jpeg/1 not implemented"
  end
end
