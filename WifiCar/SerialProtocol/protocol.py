import logging

def segment(a_str, end_mark=';', seperator=':'):
    '''
    Segment a string to a tuple, for later analysis.
    '''
    logging.getLogger(__name__).debug('segment({}, {}, {}'.format(a_str, end_mark, seperator))
    result = tuple(a_str.split(end_mark)[0].split(seperator))
    logging.getLogger(__name__).debug('segment() = {}'.format(result))
    return result

def is_response(segment_list):
    if segment_list[0] == 'RSP':
        return True
    else:
        return False

def is_ok_response(segment_list):
    if is_response(segment_list):
        if segment_list[1] == 'OK':
            return True
    return False

def temperature(segment_list):
    return float(segment_list[2])

class NotPduException(Exception):
    pass
class EmptyPduException(Exception):
    pass

class Pdu:
    END_MARK, SEPARATOR = ';', ':'

class RxPdu(Pdu):
    MINIMUM_LENGTH = 2
    def __init__(self, a_str):
        if not isinstance(a_str, str):
            raise NotPduException("type({}) is {}, not a string".format(a_str, type(a_str)))
        seg_list = a_str.split(Pdu.END_MARK)
        if len(seg_list) < 1:
            raise EmptyPduException("{} is too short to be an RxPdu".format(seg_list))
        seg_list = seg_list[0]
        if len(seg_list) == 0:  # empty string
            raise EmptyPduException("{} is too short to be an RxPdu".format(seg_list))
        seg_list = seg_list.split(Pdu.SEPARATOR)
        if len(seg_list) < RxPdu.MINIMUM_LENGTH:
            raise EmptyPduException("{} is too short to be an RxPdu".format(seg_list))
        self.type = seg_list[0]
        self.specific_fields = tuple(seg_list[1:])


class NotResponseException(Exception):
    pass
class IllegalStatusResponseException(Exception):
    pass
class MissingParameterResponseException(Exception):
    pass
class IllegalParameterResponseException(Exception):
    pass

class Response(RxPdu):
    OK, NOK, NOT_SUPPORTED, UNKNOWN = ('OK', 'NOK', 'NOT_SUPPORTED', 'UNKNOWN')
    TYPE = 'RSP'
    def __init__(self, a_str):
        super().__init__(a_str)
        if self.type != Response.TYPE:
            raise NotResponseException("type = {} is not a response".format(self.type))
        self.status = Response.UNKNOWN
        status = self.specific_fields[0]
        if status == Response.OK:
            self.status = Response.OK
        elif status == Response.NOK:
            self.status = Response.NOK
        elif status == Response.NOT_SUPPORTED:
            self.status = Response.NOT_SUPPORTED
        else:
            raise IllegalStatusResponseException("status = {} is illegal".format(status))
        self.parameters = tuple(self.specific_fields[1:])

class NotRxCmdException(Exception):
    pass
class RxCmd(RxPdu):
    EMPTY_CMD, NOK, NOT_SUPPORTED, UNKNOWN = ('', 'NOK', 'NOT_SUPPORTED', 'UNKNOWN')
    TYPE = 'CMD'
    def __init__(self, a_str):
        super().__init__(a_str)
        if self.type != RxCmd.TYPE:
            raise NotRxCmdException("type = {} is not a RxCmd".format(self.type))
        self.cmd = RxCmd.EMPTY_CMD
        if len(self.specific_fields) > 0:
            self.cmd = self.specific_fields[0]

class TemperatureResponse(Response):
    def __init__(self, a_str):
        super().__init__(a_str)
        if len(self.parameters) == 0:
            raise MissingParameterResponseException()
        try:
            self.value = float(self.parameters[0])
        except ValueError:
            raise IllegalParameterResponseException("parameter = {} is illegal for a {}".format(self.parameters[0], self.__class__))

class TxPdu(Pdu):
    MINIMUM_LENGTH = 2
    def __init__(self, pdu_type, specific_fields):
        assert isinstance(pdu_type, str)
        assert isinstance(specific_fields, tuple) or isinstance(specific_fields, list)
        self.type = pdu_type
        self.specific_fields = specific_fields
    def __str__(self):
        return self.__repr__()
    def __repr__(self):
        specific_fields = Pdu.SEPARATOR.join(self.specific_fields)
        return Pdu.END_MARK.join((Pdu.SEPARATOR.join((self.type, specific_fields)),'\n'))

class Command(TxPdu):
    TYPE = 'CMD'
    def __init__(self, specific_fields):
        super().__init__(Command.TYPE, specific_fields)

class TemperatureCommand(Command):
    CMD = 'TEMPER'
    def __init__(self):
        super().__init__((TemperatureCommand.CMD,))

class MotorForwardCommand(Command):
    def __init__(self):
        super().__init__(("FORWARD",))
class MotorBackwardCommand(Command):
    def __init__(self):
        super().__init__(("BACKWARD",))
class MotorStopCommand(Command):
    def __init__(self):
        super().__init__(("STOP",))
class MotorSpeedUpCommand(Command):
    def __init__(self):
        super().__init__(("SPEEDUP",))
class MotorSpeedDownCommand(Command):
    def __init__(self):
        super().__init__(("SPEEDDOWN",))
class MotorTurnLeftCommand(Command):
    def __init__(self):
        super().__init__(("TURNLEFT",))
class MotorTurnRightCommand(Command):
    def __init__(self):
        super().__init__(("TURNRIGHT",))

class NotDebugMsgException(Exception):
    pass

class DebugMsg(RxPdu):
    TYPE = 'DEB'
    def __init__(self, a_str):
        super().__init__(a_str)
        if self.type != DebugMsg.TYPE:
            raise NotDebugMsgException("type = {} is not a debug message".format(self.type))
        self.msg = Pdu.SEPARATOR.join(self.specific_fields)
