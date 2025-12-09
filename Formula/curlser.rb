# Homebrew formula for curlser
# This file should be placed in the homebrew-curlser repository
# Usage: brew tap kaiopiola/curlser && brew install curlser

class Curlser < Formula
  desc "Colorful HTTP client for terminal with automatic formatting"
  homepage "https://github.com/kaiopiola/curlser"
  url "https://github.com/kaiopiola/curlser/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "3b7b79989301988a4ae9ed421af20218511f1963faed2035f826590cb5f3bc96"
  license "MIT"

  depends_on "curl"

  def install
    system "make"
    bin.install "bin/curlser"
  end

  test do
    # Basic test - verify the binary runs
    assert_match "curlser", shell_output("#{bin}/curlser --help 2>&1", 1)
  end
end
