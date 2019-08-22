defmodule Pixels do
  @moduledoc """
  Elixir module to decode pixel data from image files.

  Currently only PNG files are supported.
  """

  defstruct width: 0, height: 0, data: nil

  @typedoc """
  The main struct for the image data.

  Contains fields `width`, `height`, and `data`; `data` is a binary
  which contains the decoded RGBA data for the image.

  """
  @type t :: %__MODULE__{width: integer, height: integer, data: binary}

  @doc """
  Decode a PNG image from a file
  """
  @spec read_file(filename :: String.t()) ::
          {:ok, Pixels.t()} | {:error, :enoent} | {:error, :invalid_data}
  def read_file(filename) do
    with {:ok, data} <- File.read(filename) do
      read(data)
    end
  end

  @doc """
  Decode a PNG image from raw binary input data
  """
  @spec read(data :: binary()) ::
          {:ok, Pixels.t()} | {:error, :invalid_data}
  def read(data) do
    case Pixels.Identify.identify(data) do
      :jpeg ->
        Pixels.Nif.decode_jpeg(data)

      :png ->
        Pixels.Nif.decode_png(data)

      :unknown ->
        {:error, :invalid_data}
    end
    |> process_result()
  end

  defp process_result({:error, reason}) do
    {:error, reason}
  end

  defp process_result({:error, 27, _message}) do
    {:error, :invalid_data}
  end

  defp process_result({:error, _code, message}) do
    raise RuntimeError, to_string(message)
  end

  defp process_result({width, height, data}) do
    {:ok, %Pixels{width: width, height: height, data: data}}
  end
end
