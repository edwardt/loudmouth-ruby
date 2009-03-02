require File.dirname(__FILE__) + '/spec_helper'
`cd #{File.dirname(__FILE__) + '/..'} && ruby extconf.rb configure && make clean && make`
SO_PATH = File.dirname(__FILE__) + '/../loudmouth.so'
if File.exist?(SO_PATH)
  require SO_PATH
else
  require File.dirname(__FILE__) + '/../loudmouth.bundle'
end
require 'eventmachine'

module Dispatcher
  def notify_readable
    notification = LM::Sink.notification
    notification.target.call(notification.data) if notification.target
  end
end

describe "Evented LM::Message" do

  it 'should send an iq and receive a reply' do
    conn = LM::EventedConnection.new
    conn.jid = 'vertebra-client@localhost'

    conn.set_disconnect_handler do |reason|
      puts "Disconnected"
      EM.stop
    end

    conn.open do |result|
      if result
        conn.authenticate('vertebra-client', 'testing', "agent") do |auth_result|
          unless auth_result
            puts "Failed to authenticate"
          else
            puts "Authenticated"
            m = LM::Message.new('vertebra-client@localhost/agent', LM::MessageType::IQ)
            m.node.value = "<op></op>"
            conn.send(m) do |answer|
              @answer_from_block = answer
              @answer_from_block.should_not be(nil)
            end
            conn.close
          end
        end
      else
        puts "Failed to connect"
        EM.stop
      end
    end

    EM.run do
      EM.attach(LM::Sink.file_descriptor, Dispatcher)
    end

  end

end
