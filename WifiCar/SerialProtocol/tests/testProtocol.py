import unittest
from ..protocol import RxPdu, Response, TemperatureResponse, TxPdu, Command, TemperatureCommand
import logging

logging.basicConfig(level=logging.DEBUG)

class TestProtocol(unittest.TestCase):
    def test_RxPdu(self):
        a_str = 'RSP:OK:37.4;\n'
        pdu = RxPdu(a_str)
        assert pdu.type == 'RSP'
        assert pdu.specific_fields == ('OK', '37.4')
    def test_Response(self):
        a_str = 'RSP:OK:37.4;\n'
        response = Response(a_str)
        assert response.status == 'OK'
        assert response.parameters == ('37.4',)
    def test_TemperatureResponse(self):
        a_str = 'RSP:OK:37.4;\n'
        t = TemperatureResponse(a_str)
        assert abs(t.value - 37.4) < 0.00001

    def test_TxPdU(self):
        p = TxPdu(pdu_type='CMD', specific_fields=('TEMPER',))
        assert str(p) == 'CMD:TEMPER;\n'
        p = TxPdu('CMD', ())
        assert str(p) == 'CMD:;\n'
    def test_Command(self):
        p = Command(specific_fields=('TEMPER',))
        assert str(p) == 'CMD:TEMPER;\n'
        p = Command(())
        assert str(p) == 'CMD:;\n'
    def test_TemperatureCommand(self):
        p = TemperatureCommand()
        assert str(p) == 'CMD:TEMPER;\n'