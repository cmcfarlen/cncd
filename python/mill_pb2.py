# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)



DESCRIPTOR = descriptor.FileDescriptor(
  name='mill.proto',
  package='mill',
  serialized_pb='\n\nmill.proto\x12\x04mill\"\xf5\x01\n\x07\x43ommand\x12\'\n\x04type\x18\x01 \x01(\x0e\x32\x19.mill.Command.CommandType\x12\t\n\x01x\x18\x02 \x01(\x01\x12\t\n\x01y\x18\x03 \x01(\x01\x12\t\n\x01z\x18\x04 \x01(\x01\x12\t\n\x01v\x18\x05 \x01(\x01\x12\t\n\x01\x61\x18\x06 \x01(\x01\x12\t\n\x01i\x18\x07 \x01(\x01\x12\t\n\x01j\x18\x08 \x01(\x01\x12\t\n\x01k\x18\t \x01(\x01\"i\n\x0b\x43ommandType\x12\t\n\x05PAUSE\x10\x00\x12\x08\n\x04\x46\x45\x45\x44\x10\x01\x12\n\n\x06\x46\x45\x45\x44TO\x10\x02\x12\t\n\x05RAPID\x10\x03\x12\x0b\n\x07RAPIDTO\x10\x04\x12\t\n\x05\x41RCTO\x10\x05\x12\x08\n\x04ZERO\x10\x06\x12\x0c\n\x08STARTPOS\x10\x07\"-\n\x0b\x43ommandList\x12\x1e\n\x07\x63ommand\x18\x01 \x03(\x0b\x32\r.mill.Command\"!\n\x0f\x43ommandReceived\x12\x0e\n\x06number\x18\x01 \x01(\x05\"!\n\x0f\x43ommandComplete\x12\x0e\n\x06number\x18\x01 \x01(\x05\"\xb5\x02\n\tMillState\x12\x1f\n\x01x\x18\x01 \x01(\x0b\x32\x14.mill.MillState.Axis\x12\x1f\n\x01y\x18\x02 \x01(\x0b\x32\x14.mill.MillState.Axis\x12\x1f\n\x01z\x18\x03 \x01(\x0b\x32\x14.mill.MillState.Axis\x12&\n\x0f\x63urrent_command\x18\x04 \x01(\x0b\x32\r.mill.Command\x1a\x9c\x01\n\x04\x41xis\x12\x31\n\tdirection\x18\x01 \x01(\x0e\x32\x1e.mill.MillState.Axis.Direction\x12\x10\n\x08position\x18\x02 \x01(\x01\x12\x10\n\x08velocity\x18\x03 \x01(\x01\x12\x14\n\x0c\x61\x63\x63\x65leration\x18\x04 \x01(\x01\"\'\n\tDirection\x12\x08\n\x04IDLE\x10\x00\x12\x07\n\x03POS\x10\x01\x12\x07\n\x03NEG\x10\x01\"R\n\x07Request\x12\x1e\n\x07\x63ommand\x18\x01 \x01(\x0b\x32\r.mill.Command\x12\'\n\x0c\x63ommand_list\x18\x02 \x01(\x0b\x32\x11.mill.CommandList\"\x91\x01\n\x08Response\x12/\n\x10\x63ommand_received\x18\x01 \x01(\x0b\x32\x15.mill.CommandReceived\x12/\n\x10\x63ommand_complete\x18\x02 \x01(\x0b\x32\x15.mill.CommandComplete\x12#\n\nmill_state\x18\x03 \x01(\x0b\x32\x0f.mill.MillState')



_COMMAND_COMMANDTYPE = descriptor.EnumDescriptor(
  name='CommandType',
  full_name='mill.Command.CommandType',
  filename=None,
  file=DESCRIPTOR,
  values=[
    descriptor.EnumValueDescriptor(
      name='PAUSE', index=0, number=0,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='FEED', index=1, number=1,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='FEEDTO', index=2, number=2,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='RAPID', index=3, number=3,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='RAPIDTO', index=4, number=4,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='ARCTO', index=5, number=5,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='ZERO', index=6, number=6,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='STARTPOS', index=7, number=7,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=161,
  serialized_end=266,
)

_MILLSTATE_AXIS_DIRECTION = descriptor.EnumDescriptor(
  name='Direction',
  full_name='mill.MillState.Axis.Direction',
  filename=None,
  file=DESCRIPTOR,
  values=[
    descriptor.EnumValueDescriptor(
      name='IDLE', index=0, number=0,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='POS', index=1, number=1,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='NEG', index=2, number=1,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=656,
  serialized_end=695,
)


_COMMAND = descriptor.Descriptor(
  name='Command',
  full_name='mill.Command',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='type', full_name='mill.Command.type', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='x', full_name='mill.Command.x', index=1,
      number=2, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='y', full_name='mill.Command.y', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='z', full_name='mill.Command.z', index=3,
      number=4, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='v', full_name='mill.Command.v', index=4,
      number=5, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='a', full_name='mill.Command.a', index=5,
      number=6, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='i', full_name='mill.Command.i', index=6,
      number=7, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='j', full_name='mill.Command.j', index=7,
      number=8, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='k', full_name='mill.Command.k', index=8,
      number=9, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _COMMAND_COMMANDTYPE,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=21,
  serialized_end=266,
)


_COMMANDLIST = descriptor.Descriptor(
  name='CommandList',
  full_name='mill.CommandList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='command', full_name='mill.CommandList.command', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=268,
  serialized_end=313,
)


_COMMANDRECEIVED = descriptor.Descriptor(
  name='CommandReceived',
  full_name='mill.CommandReceived',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='number', full_name='mill.CommandReceived.number', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=315,
  serialized_end=348,
)


_COMMANDCOMPLETE = descriptor.Descriptor(
  name='CommandComplete',
  full_name='mill.CommandComplete',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='number', full_name='mill.CommandComplete.number', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=350,
  serialized_end=383,
)


_MILLSTATE_AXIS = descriptor.Descriptor(
  name='Axis',
  full_name='mill.MillState.Axis',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='direction', full_name='mill.MillState.Axis.direction', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='position', full_name='mill.MillState.Axis.position', index=1,
      number=2, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='velocity', full_name='mill.MillState.Axis.velocity', index=2,
      number=3, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='acceleration', full_name='mill.MillState.Axis.acceleration', index=3,
      number=4, type=1, cpp_type=5, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _MILLSTATE_AXIS_DIRECTION,
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=539,
  serialized_end=695,
)

_MILLSTATE = descriptor.Descriptor(
  name='MillState',
  full_name='mill.MillState',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='x', full_name='mill.MillState.x', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='y', full_name='mill.MillState.y', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='z', full_name='mill.MillState.z', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='current_command', full_name='mill.MillState.current_command', index=3,
      number=4, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_MILLSTATE_AXIS, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=386,
  serialized_end=695,
)


_REQUEST = descriptor.Descriptor(
  name='Request',
  full_name='mill.Request',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='command', full_name='mill.Request.command', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='command_list', full_name='mill.Request.command_list', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=697,
  serialized_end=779,
)


_RESPONSE = descriptor.Descriptor(
  name='Response',
  full_name='mill.Response',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='command_received', full_name='mill.Response.command_received', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='command_complete', full_name='mill.Response.command_complete', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    descriptor.FieldDescriptor(
      name='mill_state', full_name='mill.Response.mill_state', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=782,
  serialized_end=927,
)

_COMMAND.fields_by_name['type'].enum_type = _COMMAND_COMMANDTYPE
_COMMAND_COMMANDTYPE.containing_type = _COMMAND;
_COMMANDLIST.fields_by_name['command'].message_type = _COMMAND
_MILLSTATE_AXIS.fields_by_name['direction'].enum_type = _MILLSTATE_AXIS_DIRECTION
_MILLSTATE_AXIS.containing_type = _MILLSTATE;
_MILLSTATE_AXIS_DIRECTION.containing_type = _MILLSTATE_AXIS;
_MILLSTATE.fields_by_name['x'].message_type = _MILLSTATE_AXIS
_MILLSTATE.fields_by_name['y'].message_type = _MILLSTATE_AXIS
_MILLSTATE.fields_by_name['z'].message_type = _MILLSTATE_AXIS
_MILLSTATE.fields_by_name['current_command'].message_type = _COMMAND
_REQUEST.fields_by_name['command'].message_type = _COMMAND
_REQUEST.fields_by_name['command_list'].message_type = _COMMANDLIST
_RESPONSE.fields_by_name['command_received'].message_type = _COMMANDRECEIVED
_RESPONSE.fields_by_name['command_complete'].message_type = _COMMANDCOMPLETE
_RESPONSE.fields_by_name['mill_state'].message_type = _MILLSTATE
DESCRIPTOR.message_types_by_name['Command'] = _COMMAND
DESCRIPTOR.message_types_by_name['CommandList'] = _COMMANDLIST
DESCRIPTOR.message_types_by_name['CommandReceived'] = _COMMANDRECEIVED
DESCRIPTOR.message_types_by_name['CommandComplete'] = _COMMANDCOMPLETE
DESCRIPTOR.message_types_by_name['MillState'] = _MILLSTATE
DESCRIPTOR.message_types_by_name['Request'] = _REQUEST
DESCRIPTOR.message_types_by_name['Response'] = _RESPONSE

class Command(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMAND
  
  # @@protoc_insertion_point(class_scope:mill.Command)

class CommandList(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMANDLIST
  
  # @@protoc_insertion_point(class_scope:mill.CommandList)

class CommandReceived(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMANDRECEIVED
  
  # @@protoc_insertion_point(class_scope:mill.CommandReceived)

class CommandComplete(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMANDCOMPLETE
  
  # @@protoc_insertion_point(class_scope:mill.CommandComplete)

class MillState(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  
  class Axis(message.Message):
    __metaclass__ = reflection.GeneratedProtocolMessageType
    DESCRIPTOR = _MILLSTATE_AXIS
    
    # @@protoc_insertion_point(class_scope:mill.MillState.Axis)
  DESCRIPTOR = _MILLSTATE
  
  # @@protoc_insertion_point(class_scope:mill.MillState)

class Request(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REQUEST
  
  # @@protoc_insertion_point(class_scope:mill.Request)

class Response(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RESPONSE
  
  # @@protoc_insertion_point(class_scope:mill.Response)

# @@protoc_insertion_point(module_scope)
