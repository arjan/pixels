defmodule Pixels do
  @moduledoc """
  Read pixels
  """

  defstruct width: 0, height: 0, format: nil, data: nil

  def read_file(filename) do
    with {width, height, format, data} <- Pixels.Nif.read_png_file(filename) do
      {:ok, %Pixels{width: width, height: height, format: map_format(format), data: data}}
    else
      reason -> {:error, reason}
    end
  end

  defp map_format(2), do: :rgb
  defp map_format(6), do: :rgba
end
