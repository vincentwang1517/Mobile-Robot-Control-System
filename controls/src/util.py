

class DataManager(object):
	def __init__(self):
		self._laserdata = []
		self._rencoder = 0
		self._lencoder = 0

	def laserdata(self):
		return self._laserdata

	def rencoder(self):
		return self._rencoder

	def lencoder(self):
		return self._lencoder

	def laserdata(self, new_laserdata):
		self._laserdata = new_laserdata

	def rencoder(self, new_rencoder):
		self._rencoder = new_rencoder

	def lencoder(self, new_lencoder):
		self._lencoder = new_lencoder