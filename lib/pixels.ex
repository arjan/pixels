defmodule Pixels do
  @moduledoc """
  Read pixels
  """

  defstruct width: 0, height: 0, data: nil

  def read_file(filename) do
    Pixels.Nif.read_png_file(filename)
    |> process_result()
  end

  def read(buffer) do
    Pixels.Nif.read_png_buffer(buffer)
    |> process_result()
  end

  defp process_result({:error, 27, _message}) do
    {:error, :invalid_format}
  end

  defp process_result({:error, 78, _message}) do
    {:error, :not_found}
  end

  defp process_result({:error, _code, message}) do
    raise RuntimeError, to_string(message)
  end

  defp process_result({width, height, data}) do
    {:ok, %Pixels{width: width, height: height, data: data}}
  end
end
