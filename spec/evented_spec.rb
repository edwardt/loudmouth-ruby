require File.dirname(__FILE__) + '/spec_helper'
require 'eventmachine'
SO_PATH = File.dirname(__FILE__) + '/../loudmouth.so'
if File.exist?(SO_PATH)
  require SO_PATH
else
  require File.dirname(__FILE__) + '/../loudmouth.bundle'
end

$conn = LM::EventedConnection.new
$conn.jid = 'vertebra-client@localhost'

module CallbackHandler

  def notify_readable
    buf = ""
    $pipe_read.read(1, buf)
    notification = LM::Sink.notification
    case notification.kind
    when LM::CB_CONN_OPEN
      handle_connection_open notification.data
    when LM::CB_AUTH
      handle_authentication notification.data
    when LM::CB_MSG
      handle_message notification.data
    when LM::CB_REPLY
      handle_reply notification.data
    when LM::CB_DISCONNECT
      handle_disconnect notification.data
    end
  end

  def handle_connection_open(success)
    if success
      $conn.authenticate('vertebra-client', 'testing', "agent") {}
    else
      puts "Connection failed"
    end
  end

  def handle_authentication(success)
    if success
      m = LM::Message.new('vertebra-client@localhost/agent', LM::MessageType::IQ)
      m.node.value = "<op></op>"
      $conn.send(m) {}
    else
      puts "Failed to authenticate"
    end
  end

  def handle_message message
  end

  def handle_reply message
    puts "ANSWER #{message.inspect}"
    $conn.close
    EM.stop
  end

  def handle_disconnect(reason)
    puts "Disconnected (#{reason})"
    EM.stop
  end

end

EM.run do
  $pipe_read = IO.for_fd(LM::Sink.file_descriptor, "r")
  EM.attach(LM::Sink.file_descriptor, CallbackHandler)
  $conn.open {}
end
