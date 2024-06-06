require 'mongoid'

class Policy
   include Mongoid::Document

#   embeds_many :accesses
  
   field :name, type: String
   field :modifyDate, type: Time
   field :users, type: Array
   
end
