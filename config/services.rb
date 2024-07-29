require 'service_manager'

ServiceManager.define_service 'nodes' do |s|
   s.start_cmd = 'build/nodes --test --logLevel=trace'
   s.loaded_cue = /Local REP/
   s.cwd = Dir.pwd
   s.pid_file = 'nodes.pid'
end
