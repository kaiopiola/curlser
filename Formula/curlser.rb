# Homebrew formula for curlser
# This file should be placed in the homebrew-curlser repository
# Usage: brew tap kaiopiola/curlser && brew install curlser

class Curlser < Formula
  desc "Colorful HTTP client for terminal with automatic formatting"
  homepage "https://github.com/kaiopiola/curlser"
  url "https://github.com/kaiopiola/curlser/archive/refs/tags/v1.0.0.tar.gz"
  sha256 "0019dfc4b32d63c1392aa264aed2253c1e0c2fb09216f8e2cc269bbfb8bb49b5"
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
