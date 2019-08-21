defmodule Pixels do
  @moduledoc """
  Read pixels
  """

  defstruct width: 0, height: 0, data: nil

  def read_file(filename) do
    case Pixels.Nif.read_png_file(filename) do
      {:error, 27, _message} ->
        {:error, :invalid_format}

      {:error, 78, _message} ->
        {:error, :not_found}

      {:error, _code, message} ->
        raise RuntimeError, to_string(message)

      {width, height, data} ->
        {:ok, %Pixels{width: width, height: height, data: data}}
    end
  end
end
