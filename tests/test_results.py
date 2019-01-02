from unittest import TestCase
from json import load
from os.path import join, dirname 

class TestResults(TestCase):

    def test_result(self):

        expected_results = load(open(join(dirname(__file__), "expected_result.json"), 'r'))
        results = load(open(join(dirname(__file__), "result.json"), 'r'))
        
        self.assertAlmostEqual(expected_results['error'], results['error'], delta=expected_results['error']*1e-6)

        for i, parameter in enumerate(expected_results['parameters']):
            self.assertAlmostEqual(parameter['value'], results['parameters'][i]['value'], delta=parameter['value']*1e-6)
