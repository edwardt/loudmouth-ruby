require 'rake/clean'
require 'rake/gempackagetask'

CLEAN.include "ext/**/*.{bundle,o,so}"
CLOBBER.include "ext/Makefile", "ext/mkmf.log", "pkg/**/*"

gemspec = Gem::Specification.new do |s|
  s.name = 'loudmouth-ruby'
  s.homepage = 'http://github.com/engineyard/loudmouth-ruby'
  s.summary = 'Ruby bindings to the Loudmouth XMPP library'
  s.version = '0.3.0'
  s.authors = ['Joshua Sierles',
               'Mikael Hallendal',
               'Raphael Simon',
               'Samuel Tesla',
               'Kirk Haines']
  s.email = 'stesla@engineyard.com'
  s.extensions = ['ext/extconf.rb']
  s.files = FileList['README', 'COPYING', 'Rakefile',
                     '{examples,spec}/**/*', 'ext/*.{c,h,rb}']
  s.has_rdoc = false
  s.require_paths << 'ext'
  s.requirements << 'none'
end

Rake::GemPackageTask.new(gemspec) do |pkg|
  pkg.need_tar = true
end

task :default => 'ext:build'

namespace :ext do
  desc 'Build C extension'
  task :build => [:clean, :make]

  desc 'Make extension'
  task :make => [:makefile] do
    chdir 'ext' do
      sh 'make'
    end
  end

  desc 'Make Makefile'
  task :makefile do
    chdir 'ext' do
      ruby 'extconf.rb'
    end
  end
end
