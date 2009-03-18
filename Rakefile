require 'rake/clean'
require 'rake/gempackagetask'

CLEAN.include "ext/**/*.{bundle,o,so}"
CLOBBER.include "ext/Makefile", "ext/mkmf.log", "pkg/**/*"

GEM = 'loudmouth-ruby'
GEM_VERSION = '0.3.0'
AUTHORS = ['Joshua Sierles',
           'Mikael Hallendal',
           'Raphael Simon',
           'Samuel Tesla',
           'Kirk Haines']
EMAIL = 'loudmouth-dev@googlegroups.com'
HOMEPAGE = 'http://github.com/engineyard/loudmouth-ruby'
SUMMARY = 'Ruby bindings to the Loudmouth XMPP library'

gemspec = Gem::Specification.new do |s|
  s.name = GEM
  s.homepage = HOMEPAGE
  s.summary = SUMMARY
  s.version = GEM_VERSION
  s.authors = AUTHORS
  s.email = EMAIL
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
