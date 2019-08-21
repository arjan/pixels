defmodule Pixels.MixProject do
  use Mix.Project

  def project do
    [
      app: :pixels,
      compilers: [:elixir_make] ++ Mix.compilers(),
      version: File.read!("VERSION"),
      elixir: "~> 1.7",
      elixirc_options: [warnings_as_errors: true],
      description: description(),
      package: package(),
      source_url: "https://github.com/arjan/pixels",
      homepage_url: "https://github.com/arjan/pixels",
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  defp description do
    "Elixir NIF to read image data from PNG files"
  end

  defp package do
    %{
      files: ["lib", "mix.exs", "*.md", "LICENSE", "VERSION"],
      maintainers: ["Arjan Scherpenisse"],
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/arjan/pixels"}
    }
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
      {:elixir_make, "~> 0.6.0", runtime: false},
      {:ex_doc, "~> 0.21", runtime: false, only: :dev}
    ]
  end
end
