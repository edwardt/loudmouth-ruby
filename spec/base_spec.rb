require File.dirname(__FILE__) + '/spec_helper'
`cd #{File.dirname(__FILE__) + '/..'} && ruby extconf.rb configure && make clean && make`
SO_PATH = File.dirname(__FILE__) + '/../loudmouth.so'
if File.exist?(SO_PATH)
  require SO_PATH
else
  require File.dirname(__FILE__) + '/../loudmouth.bundle'
end
require 'glib2'

describe "Basic LM::Message" do

  it 'should send an iq and receive a reply' do
    main_loop = GLib::MainLoop.new
    conn = LM::Connection.new
    conn.jid = 'vertebra-client@localhost'

    conn.set_disconnect_handler do |reason|
      main_loop.quit
    end

    conn.open do |result|
      if result
        conn.authenticate('vertebra-client', 'testing', "agent") do |auth_result|
          unless auth_result
            puts "Failed to authenticate"
          else
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
        main_loop.quit
      end
    end
    
    main_loop.run
    
  end

end
