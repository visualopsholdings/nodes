require 'service_manager'

ServiceManager.define_service 'zmqchat' do |s|
   s.start_cmd = 'build/ZMQCHAT --logLevel=trace'
   s.loaded_cue = /Local REP/
   s.cwd = Dir.pwd
   s.pid_file = 'zmqchat.pid'
end
