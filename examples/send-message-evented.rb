# This example uses the evented binding
# The evented binding allows for non blocking handling of
# incoming messages (the loudmouth thread is not waiting
# on the ruby thread).
# It also alleviates the need for ruby-gnome2 (glib2 gem)

require 'loudmouth'
require 'rubygems'
require 'eventmachine'

puts "Enter your JID: "
jid = gets.chomp

puts "Enter connect host: "
host = gets.chomp

puts "Enter your password: "
password = gets.chomp

if /(.+)@(.+)/ =~ jid
  login = $1
end

puts "Logging in as '#{login}' to '#{host}'"

conn = LM::EventedConnection.new(host)
conn.jid = jid
conn.ssl = LM::SSL.new
conn.ssl.use_starttls = true
conn.ssl.require_starttls = true

recipient = ""

conn.set_disconnect_handler do |reason|
  puts "Disconnected"
  EM.stop
end

conn.open do |result|
  puts "Connection open block"
  if result
    puts "Connection opened correctly"
    conn.authenticate(login, password, "Test") do |auth_result|
      unless auth_result
        puts "Failed to authenticate"
      end
      authenticated_cb(conn)
      EM.stop
    end
  else
    puts "Failed to connect"
    EM.stop
  end
end

def authenticated_cb(conn)
  puts "Authenticated!"
  puts "Who do you want to message: "
  recipient = gets.chomp
  
  puts "Enter message: "
  body = gets.chomp
  
  m = LM::Message.new(recipient, LM::MessageType::MESSAGE)
  m.node.add_child('body', body)
  
  conn.send(m)
  conn.close
  
  puts "Message sent to #{recipient}"
end

module Dispatcher
  def notify_readable
    cb = LM::Sink.notification
    cb.target.call(cb.data) if cb.target
  end
end

EM.run do
  EM.attach(LM::Sink.file_descriptor, Dispatcher)
end

puts "Quitting"
