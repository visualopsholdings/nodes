
Given(/^there are users:$/) do |users|

   users.hashes.each do |s|
   
      user = FactoryBot.build(:user)
      user.name = s[:name]
      user.admin = s[:admin]
      
      if s[:id] && s[:id].length > 0
         user._id = s[:id]
      end
      
      if s[:fullname] && s[:fullname].length > 0
         user.fullname = s[:fullname]
      end
            
      user.save
      
   end
   
end

Given(/^there are policies:$/) do |table|
   table.hashes.each do |s|
      policy = FactoryBot.build(:policy)
      policy.name = s[:name]
      
      if s[:id] && s[:id].length > 0
         policy._id = s[:id]
      end

      if s[:users] && s[:users].length > 0
         policy.users = []
         s[:users].split(" ").each do |u|
            policy.users.push(User.where(name: u).first._id.to_s)
         end
      end

      policy.save
   end
end

Given(/^there are streams:$/) do |table|

   table.hashes.each do |s|
      stream = FactoryBot.build(:stream)
      
      if s[:id] && s[:id].length > 0
         stream._id = s[:id]
      end
      
      if s[:modifyDate] && s[:modifyDate].length > 0
         stream.modifyDate = parse_date(s[:modifyDate])
      end

      stream.name = s[:name]

      if s[:policy] && s[:policy].length > 0
         stream.policy = Policy.where(name: s[:policy]).first._id.to_s
      end

      stream.active = true
      stream.save
   end
end
