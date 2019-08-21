defmodule Pixels.MixProject do
  use Mix.Project

  def project do
    # for {k, v} <- System.get_env() do
    #   IO.puts "#{k} - #{v}"
    # end

    [
      app: :pixels,
      compilers: [:elixir_make] ++ Mix.compilers(),
      version: "0.1.0",
      elixir: "~> 1.9",
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:elixir_make, "~> 0.6.0", runtime: false}
    ]
  end
end
