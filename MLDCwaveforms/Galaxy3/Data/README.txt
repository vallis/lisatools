This directory contains the binary population catalogs provided by Gijs Nelemans (files AMCVn_GWR_MLDC.dat and dwd_GWR_MLDC.dat, downloaded on demand by the master-install.py script), which contain the parameters of detached and interacting systems produced by the population synthesis codes described in \cite{Nelemans:2001hp, Nelemans:2003ha}.

The catalogs list the component masses M, m of the heavier and lighter component of each system, the initial orbital period P_0 and its first time derivative \dot{P}_0, the time derivative of the donor mass \dot{m}, the galactic latitude and longitude, and the distance d to the system.

Some of the information in the population files is redundant since the rate of orbital evolution can be computed from the masses, orbital period and mass transfer rate by

\begin{equation}
\frac{\dot{f}}{f} = -\frac{\dot{P}}{P} = 
                    \frac{96}{3}(\pi f)^{8/3}{\cal M}^{5/3}  
                    - 3 \left(1 - \frac{m}{M}\right) \frac{\dot{m}}{m}
\end{equation}

where f=2/P is the gravitational wave frequency and
{\cal M} = (mM)^{3/5}/(m+M)^{1/5} is the chirp mass.

Also, file Verification.dat specifies the parameters of verification binaries as rows of f [Hz], \dot{f} [Hz/Second], \beta [Radian], \lambda [Radian], and A [dimensionless].

\bibitem{Nelemans:2001hp}
  G.~Nelemans, L.~R.~Yungelson and S.~F.~Portegies Zwart,
  %``The gravitational wave signal from the galactic disk population of binaries containing two compact objects,''
  arXiv:astro-ph/0105221.

\bibitem{Nelemans:2003ha}
  G.~Nelemans, L.~R.~Yungelson and S.~F.~Portegies Zwart,
  %``Short-period AM CVn systems as optical, X-ray and gravitational wave sources,''
  Mon.\ Not.\ Roy.\ Astron.\ Soc.\  {\bf 349}, 181 (2004)
  [arXiv:astro-ph/0312193].
