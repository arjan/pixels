defmodule Pixels.Nif do
 @on_load :load_nifs

  def load_nifs do
    file = :filename.join(:code.priv_dir(:pixels), 'pixels_nif')
    :ok = :erlang.load_nif(file, 0)
  end

  def read_png_file(_filename) do
    raise "NIF read_png_file/1 not implemented"
  end
    
end

