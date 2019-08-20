defmodule Mix.Tasks.Compile.Nif do
  def run(_args) do
    {result, _} = System.cmd("make", [])
    IO.binwrite(result)
  end
end

defmodule Pixels.MixProject do
  use Mix.Project

  def project do
    # for {k, v} <- System.get_env() do
    #   IO.puts "#{k} - #{v}"
    # end

    [
      app: :pixels,
      compilers: [:nif] ++ Mix.compilers(),
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
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"}
    ]
  end
end
